use std::sync::{Arc, Mutex};

use actix_web::{App, HttpServer, web};
use tokio::sync::broadcast::Receiver;

use crate::{
    http_handlers::hello_world::manual_hello_world_handler, state::storage_unit::TcpStorage,
};

pub struct AppState {
    pub tcp_storage_state: Arc<Mutex<TcpStorage>>,
}

/// client contacts master(self) to get things done through this service
pub async fn http_main(
    mut shutdown_receiver: Receiver<()>,
    tcp_storage_state: Arc<Mutex<TcpStorage>>,
) -> Result<(), String> {
    let res = HttpServer::new(move || {
        App::new()
            .app_data(web::Data::new(AppState {
                tcp_storage_state: Arc::clone(&tcp_storage_state),
            }))
            .route("/", web::get().to(manual_hello_world_handler))
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
