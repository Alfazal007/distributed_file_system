use actix_web::{App, HttpResponse, HttpServer, Responder, web};
use tokio::sync::broadcast::Receiver;

async fn manual_hello() -> impl Responder {
    HttpResponse::Ok().body("Test route")
}

/// client contacts master(self) to get things done through this service
pub async fn http_main(mut shutdown_receiver: Receiver<()>) -> Result<(), String> {
    let res = HttpServer::new(|| App::new().route("/", web::get().to(manual_hello)))
        .bind(("127.0.0.1", 8000))
        .map_err(|e| format!("Failed to bind HTTP server: {}", e))?
        .run();

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
