use actix_web::{HttpResponse, Responder, web};

use crate::http_connection_to_client::http_main::AppState;

pub async fn manual_hello_world_handler(data: web::Data<AppState>) -> impl Responder {
    println!("{:?}", data.tcp_storage_state.lock().unwrap().connections);
    HttpResponse::Ok().body("hello world")
}
