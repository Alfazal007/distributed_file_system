use actix_web::{HttpResponse, Responder, web};
use serde::Deserialize;

use crate::http_connection_to_client::http_main::AppState;

#[derive(Deserialize)]
pub struct GetChunkInput {
    file_name: String,
}

pub async fn get_list_chunks(
    item: web::Json<GetChunkInput>,
    data: web::Data<AppState>,
) -> impl Responder {
    let location = data
        .tcp_storage_state
        .lock()
        .unwrap()
        .get_chunks_for_file(&item.0.file_name);
    HttpResponse::Ok().json(location)
}
