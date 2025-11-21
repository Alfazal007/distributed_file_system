import { protocol } from "../constants"
import { tryCatch } from "../helpers/tryCatch"
import fs from "fs"
import axios from "axios"
import { getChunkServerToAddChunkTo } from "../master/getChunkServerToInsertTo"

export async function insertChunk(
    absoluteFilePath: string,
    fileName: string
): Promise<boolean> {
    const CHUNK_SIZE = 8 * 1024
    const fileBuffer = fs.readFileSync(absoluteFilePath)
    const totalChunks = Math.ceil(fileBuffer.length / CHUNK_SIZE)

    for (let chunkId = 0; chunkId < totalChunks; chunkId++) {
        const start = chunkId * CHUNK_SIZE
        const end = Math.min(start + CHUNK_SIZE, fileBuffer.length)
        const chunk = fileBuffer.subarray(start, end)

        const locations = await getChunkServerToAddChunkTo() as string[]

        for (const location of locations) {
            const url = `${protocol}://${location}:8002/new-chunk`
            let uploaded = false

            while (!uploaded) {
                const response = await tryCatch(
                    axios.post(url, chunk, {
                        headers: {
                            'X-Filename': fileName,
                            'X-Chunk-ID': String(chunkId),
                        },
                    })
                )

                if (response.error) {
                    continue
                }

                if (response.data.status === 200) {
                    uploaded = true
                }
            }
        }
    }

    return true
}
