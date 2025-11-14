import { MASTERADDR } from "../constants";
import { tryCatch } from "../helpers/tryCatch";
import axios from "axios"

export async function getChunkServerToAddToChunkTo(): Promise<string | null> {
    const response = await tryCatch(axios.get(`${MASTERADDR}/chunk-location`))
    if (response.data) {
        // TODO:: complete this
    }
    return null
}
