import { MASTERADDR } from "../constants";
import { tryCatch } from "../helpers/tryCatch";
import axios from "axios"

export async function getMultipleChunkLocations(filename: string, chunkIds: number[]): Promise<string[][] | null> {
    const response = await tryCatch(axios.post(`${MASTERADDR}/chunk-location-multiple`, {
        "file_name": filename,
        "chunk_ids": chunkIds
    }))
    if (response.data) {
        return response.data.data
    }
    return null
}
