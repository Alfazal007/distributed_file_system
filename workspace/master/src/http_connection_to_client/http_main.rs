use std::time::Duration;

use tokio::time::sleep;

pub async fn http_main() {
    sleep(Duration::from_secs(4)).await;
    println!("This is the main function of http connection");
}
