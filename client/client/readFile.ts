import axios from "axios";
import fs from "fs";
import path from "path";
import { tryCatch } from "../helpers/tryCatch";
import { MASTERADDR, protocol } from "../constants";
import { getMultipleChunkLocations } from "../master/multipleChunkLocation";

export async function readFile(filepath: string): Promise<boolean> {
    const outputPath = path.join(__dirname, `../finalReadFile/${filepath}`);
    try {
        fs.mkdirSync(path.dirname(outputPath), { recursive: true });
        fs.writeFileSync(outputPath, "");
    } catch {
        return false;
    }
    const chunksResult = await tryCatch(
        axios.post(`${MASTERADDR}/chunks`, { file_name: filepath })
    );
    if (chunksResult.error) return false;
    const chunkIds: number[] = chunksResult.data.data;
    const locations = await getMultipleChunkLocations(filepath, chunkIds);
    if (!locations || locations.length === 0) return false;
    if (locations.some((loc) => !loc || loc.length === 0)) return false;
    for (let i = 0; i < locations.length; i++) {
        const chunk = chunkIds[i];
        const replicas = locations[i] as string[];
        const randomIndex = Math.floor(Math.random() * replicas.length);
        const nodeHost = replicas[randomIndex];
        const url = `${protocol}://${nodeHost}:8002/chunk`;
        const dataResult = await tryCatch(
            axios.get(url, {
                headers: {
                    "X-Filename": filepath,
                    "X-Chunk-ID": String(chunk),
                },
                responseType: "arraybuffer",
            })
        );
        if (dataResult.error) return false;
        const buf = Buffer.from(dataResult.data.data)
        try {
            fs.appendFileSync(outputPath, buf);
        } catch {
            return false;
        }
    }
    return true;
}
