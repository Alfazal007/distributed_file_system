use actix_web::{HttpResponse, Responder, web};

use crate::http_connection_to_client::http_main::AppState;

pub async fn show_available_files(data: web::Data<AppState>) -> impl Responder {
    let files = data.tcp_storage_state.lock().unwrap().get_file_list();
    return HttpResponse::Ok().json(files);
}
