use actix_web::{HttpResponse, Responder, web};
use serde::Deserialize;

use crate::http_connection_to_client::http_main::AppState;

#[derive(Deserialize)]
pub struct GetFileLocationsInput {
    file_name: String,
}

pub async fn get_file_location(
    item: web::Json<GetFileLocationsInput>,
    data: web::Data<AppState>,
) -> impl Responder {
    let locations = data
        .tcp_storage_state
        .lock()
        .unwrap()
        .get_file_locations(&item.0.file_name);
    HttpResponse::Ok().json(locations)
}
