use actix_web::{HttpResponse, Responder, web};
use serde::Deserialize;

use crate::http_connection_to_client::http_main::AppState;

#[derive(Deserialize)]
pub struct GetMultipleChunkLocationInput {
    file_name: String,
    chunk_ids: Vec<i32>,
}

pub async fn get_chunk_location_addr_multiple(
    item: web::Json<GetMultipleChunkLocationInput>,
    data: web::Data<AppState>,
) -> impl Responder {
    let location = data
        .tcp_storage_state
        .lock()
        .unwrap()
        .get_multiple_chunks(&item.0.file_name, item.0.chunk_ids);
    HttpResponse::Ok().json(location)
}
