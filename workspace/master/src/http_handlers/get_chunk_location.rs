use actix_web::{HttpResponse, Responder, web};
use serde::Deserialize;

use crate::http_connection_to_client::http_main::AppState;

#[derive(Deserialize)]
pub struct GetChunkLocationInput {
    file_name: String,
    chunk_id: i32,
}

pub async fn get_chunk_location_addr(
    item: web::Json<GetChunkLocationInput>,
    data: web::Data<AppState>,
) -> impl Responder {
    let location = data
        .tcp_storage_state
        .lock()
        .unwrap()
        .get_location(&item.0.file_name, item.0.chunk_id);
    HttpResponse::Ok().json(location)
}
