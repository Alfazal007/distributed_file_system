use actix_web::{HttpResponse, Responder, web};

use crate::http_connection_to_client::http_main::AppState;

pub async fn return_server_to_write_chunk(data: web::Data<AppState>) -> impl Responder {
    let index_to_write = data
        .tcp_storage_state
        .lock()
        .unwrap()
        .increment_index_and_return();
    return HttpResponse::Ok().json(index_to_write);
}
