use crate::{
    http_connection_to_client::http_main::http_main, tcp_connection_to_storage::tcp_main::tcp_main,
};

pub mod http_connection_to_client;
pub mod tcp_connection_to_storage;

#[tokio::main]
async fn main() {
    let join_handle_vecs = vec![tokio::spawn(tcp_main()), tokio::spawn(http_main())];
    let _ = futures::future::join_all(join_handle_vecs).await;
}
