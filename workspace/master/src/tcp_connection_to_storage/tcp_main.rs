use std::time::Duration;

use tokio::time::sleep;

pub async fn tcp_main() {
    sleep(Duration::from_secs(6)).await;
    println!("This is the main function of tcp connection");
}
