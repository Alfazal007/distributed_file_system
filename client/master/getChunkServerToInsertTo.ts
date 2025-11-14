import { MASTERADDR } from "../constants";
import { tryCatch } from "../helpers/tryCatch";
import axios from "axios"

export async function getChunkServerToAddToChunkTo(): Promise<string | null> {
    const response = await tryCatch(axios.get(`${MASTERADDR}/server_to_write`))
    if (response.data) {
        const addressToWrite = response.data.data
        console.log({ addressToWrite })
        return addressToWrite
    }
    return null
}
