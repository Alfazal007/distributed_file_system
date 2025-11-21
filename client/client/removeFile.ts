import { MASTERADDR, protocol } from "../constants"
import { tryCatch } from "../helpers/tryCatch"
import axios from "axios"

export async function deleteFile(
    fileName: string,
): Promise<boolean> {
    const locationsUrl = `${MASTERADDR}/filelocation`
    let locationsResult = await tryCatch(axios.post(locationsUrl, {
        "file_name": fileName
    }))
    if (locationsResult.error) {
        return false
    }
    let locations: string[] = locationsResult.data.data
    for (let locationOfFile in locations) {
        const url = `${protocol}://${locationOfFile}:8002/remove-file`
        const response = await tryCatch(
            axios.delete(url, {
                headers: {
                    'X-Filename': fileName,
                },
            })
        )
        if (response.error || response.data.status != 200) {
            return false
        }
    }
    return true
}
