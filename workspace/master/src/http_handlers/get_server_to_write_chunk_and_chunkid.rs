use std::collections::HashSet;

use actix_web::{HttpResponse, Responder, web};

use crate::http_connection_to_client::http_main::AppState;

pub async fn return_server_to_write_chunk(data: web::Data<AppState>) -> impl Responder {
    let mut i = 0;
    let mut indexs_to_write = HashSet::new();
    while i < data.replication_count {
        indexs_to_write.insert(
            data.tcp_storage_state
                .lock()
                .unwrap()
                .increment_index_and_return_addr(),
        );
        i += 1;
    }
    let mut res = Vec::new();
    for index in indexs_to_write {
        if let Some(addr) = index {
            res.push(addr);
        }
    }
    return HttpResponse::Ok().json(res);
}
