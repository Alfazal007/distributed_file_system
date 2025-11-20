use std::sync::{Arc, Mutex};

use actix_web::{App, HttpServer, web};
use tokio::sync::broadcast::Receiver;

use crate::{
    http_handlers::{
        get_chunk_id_list::get_list_chunks, get_chunk_location::get_chunk_location_addr,
        get_multiple_chunk_location::get_chunk_location_addr_multiple,
        get_server_to_write_chunk_and_chunkid::return_server_to_write_chunk,
        show_available_files::show_available_files,
    },
    state::storage_unit::TcpStorage,
};

pub struct AppState {
    pub tcp_storage_state: Arc<Mutex<TcpStorage>>,
    pub replication_count: i32,
}

/// client contacts master(self) to get things done through this service
pub async fn http_main(
    mut shutdown_receiver: Receiver<()>,
    tcp_storage_state: Arc<Mutex<TcpStorage>>,
    replication_count: i32,
) -> Result<(), String> {
    let res = HttpServer::new(move || {
        App::new()
            .app_data(web::Data::new(AppState {
                tcp_storage_state: Arc::clone(&tcp_storage_state),
                replication_count,
            }))
            .route(
                "/server_to_write",
                web::get().to(return_server_to_write_chunk),
            )
            .route("/chunk-location", web::post().to(get_chunk_location_addr))
            // on it
            .route(
                "/chunk-location-multiple",
                web::post().to(get_chunk_location_addr_multiple),
            )
            .route("/chunks", web::post().to(get_list_chunks))
            .route("/file-list", web::get().to(show_available_files))
    })
    .bind(("127.0.0.1", 8000))
    .map_err(|e| format!("Failed to bind HTTP server: {}", e))?
    .run();
    println!("Http server listening on port 8000");

    tokio::select! {
        _ = shutdown_receiver.recv() => {
            println!("returning http connetion");
            return Err("Error message received, closing tcp connection".to_string());
        },
        result = res => {
            result.map_err(|e| format!("HTTP server error: {}", e))
        }
    }
}
