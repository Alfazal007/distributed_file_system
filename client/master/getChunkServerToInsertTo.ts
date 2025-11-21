import { MASTERADDR } from "../constants";
import { tryCatch } from "../helpers/tryCatch";
import axios from "axios"

export async function getChunkServerToAddChunkTo(): Promise<string[] | null> {
    const response = await tryCatch(axios.get(`${MASTERADDR}/server_to_write`))
    if (response.data) {
        const addressToWrite = response.data.data
        return addressToWrite
    }
    return null
}
