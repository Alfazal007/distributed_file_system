use crate::{
    protos::data, state::storage_unit::TcpStorage,
    tcp_connection_to_storage::loop_health_check_sender::health_check_send,
};
use prost::Message;
use std::sync::{Arc, Mutex};
use tokio::{
    io::{AsyncReadExt, BufReader},
    net::TcpListener,
};
use uuid::Uuid;

pub async fn accept_connections(
    listener: TcpListener,
    tcp_storage: Arc<Mutex<TcpStorage>>,
) -> Result<(), String> {
    loop {
        let tcp_storage_inner = Arc::clone(&tcp_storage);
        let (socket, addr) = listener
            .accept()
            .await
            .map_err(|e| format!("Failed to accept connection: {}", e))?;

        tokio::spawn(async move {
            println!("New tcp connection arrived");
            // Used to track if connection has sent the join message or not
            let shared_conn_state = Arc::new(Mutex::new(true));
            let connection_id = Uuid::new_v4().to_string();
            let (reader, writer) = socket.into_split();
            let mut buf_reader = BufReader::new(reader);
            let mut buf: Vec<u8> = Vec::new();
            tokio::spawn(health_check_send(
                writer,
                Arc::clone(&shared_conn_state),
                connection_id.clone(),
            ));
            loop {
                buf.clear();
                match buf_reader.read_u32().await {
                    Ok(size) => {
                        let mut buf = vec![0u8; size as usize];
                        match buf_reader.read_exact(&mut buf).await {
                            Ok(_) => {
                                println!("{:?}", buf);
                                if let Ok(msg) = data::MessageFromStorageToMaster::decode(&*buf) {
                                    if let Some(internal_msg) = msg.msg_type {
                                        match internal_msg {
                                                    data::message_from_storage_to_master::MsgType::Join(_) => {
                                                        tcp_storage_inner
                                                            .lock()
                                                            .expect("Deadlock in tcp message locking stage")
                                                            .insert(&connection_id, addr.ip());
                                                        *shared_conn_state.lock().unwrap() = true;
                                                    }
                                                    data::message_from_storage_to_master::MsgType::Health(
                                                        file_state,
                                                    ) => tcp_storage_inner
                                                        .lock()
                                                        .expect("Deadlock in tcp message locking stage")
                                                        .update_state(&connection_id, file_state),
                                                }
                                    }
                                } else {
                                    println!("wrong message");
                                }
                            }
                            Err(e) => {
                                println!("Error reading data from {}: {}", addr, e);
                                break;
                            }
                        }
                    }
                    Err(e) => {
                        println!("Error reading size from {}: {}", addr, e);
                        break;
                    }
                }
            }
            let mut update_shared_state = shared_conn_state.lock().unwrap();
            *update_shared_state = false;
            tcp_storage_inner
                .lock()
                .expect("Deadlock in tcp message locking stage")
                .remove(&connection_id);
        });
    }
}
