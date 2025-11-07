use std::{
    sync::{Arc, Mutex},
    time::Duration,
};
use tokio::{io::AsyncWriteExt, net::tcp::OwnedWriteHalf, time::interval};

pub async fn health_check_send(mut writer: OwnedWriteHalf, shared_conn_state: Arc<Mutex<bool>>) {
    let mut interval = interval(Duration::from_secs(10));
    loop {
        interval.tick().await;
        if *shared_conn_state.lock().unwrap() == false {
            return;
        }
        writer.write("T\n".as_bytes()).await.unwrap();
        println!("flush");
        writer.flush().await.unwrap();
    }
}
