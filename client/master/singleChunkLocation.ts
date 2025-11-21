import { MASTERADDR } from "../constants";
import { tryCatch } from "../helpers/tryCatch";
import axios from "axios"

export async function getSingleChunkLocations(filename: string, chunkId: number): Promise<string[] | null> {
    const response = await tryCatch(axios.post(`${MASTERADDR}/chunk-location`, {
        "file_name": filename,
        "chunk_id": chunkId
    }))
    if (response.data) {
        return response.data.data
    }
    return null
}
