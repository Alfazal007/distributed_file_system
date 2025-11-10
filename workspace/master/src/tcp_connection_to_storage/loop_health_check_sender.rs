use prost::Message;
use std::{
    sync::{Arc, Mutex},
    time::Duration,
};
use tokio::{io::AsyncWriteExt, net::tcp::OwnedWriteHalf, time::interval};

use crate::protos::data::MessageFromMasterToStorage;

pub async fn health_check_send(
    mut writer: OwnedWriteHalf,
    shared_conn_state: Arc<Mutex<bool>>,
    connection_id: String,
) {
    let message_to_send = MessageFromMasterToStorage { id: connection_id };
    let mut encoded_message = MessageFromMasterToStorage::encode_to_vec(&message_to_send);
    encoded_message.push(b'\n');
    let mut interval = interval(Duration::from_secs(10));
    loop {
        interval.tick().await;
        if *shared_conn_state.lock().unwrap() == false {
            return;
        }
        writer.write(&encoded_message[..]).await.unwrap();
        println!("flushing");
        writer.flush().await.unwrap();
    }
}
