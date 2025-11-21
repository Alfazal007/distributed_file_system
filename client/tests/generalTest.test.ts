import { expect, test, } from "bun:test";
import { getChunkServerToAddChunkTo } from "../master/getChunkServerToInsertTo";
import { getSingleChunkLocations } from "../master/singleChunkLocation";
import { getMultipleChunkLocations } from "../master/multipleChunkLocation";
import path from "path"
import { tryCatch } from "../helpers/tryCatch";
import { deleteFile } from "../client/removeFile";
import { insertFile } from "../client/insertFile";
import { readFile } from "../client/readFile";

let serverToWriteTo: string[]

test("testing get chunkserver to write to", async () => {
    const serverToWrite = await getChunkServerToAddChunkTo()
    expect(serverToWrite).toEqual(["127.0.0.1"])
    if (serverToWrite)
        serverToWriteTo = serverToWrite
})

test("testing upload file", async () => {
    let fileName = "tester.pdf"
    let absolutePath = path.join(__dirname, `../file/${fileName}`)
    let uploadResult = await tryCatch(insertFile(absolutePath, fileName))
    expect(uploadResult.data).toBe(true)
    await new Promise((resolve) => setTimeout(() => resolve(true), 10000)) // so master can reflect changes
})

test("testing get single chunk location", async () => {
    let locations = await getSingleChunkLocations("invalid", 68)
    expect(locations).toEqual([])
    locations = await getSingleChunkLocations("tester.pdf", 1)
    expect(locations).toEqual(["127.0.0.1"])
})

test("testing get multiple chunks location", async () => {
    let locations = await getMultipleChunkLocations("invalid", [1, 2, 3, 4, 68])
    expect(locations).toEqual([[], [], [], [], []])
    locations = await getMultipleChunkLocations("tester.pdf", [1, 34, 3])
    expect(locations).toEqual([["127.0.0.1"], [], ["127.0.0.1"]])
})

test("read file", async () => {
    let locations = await readFile("tester.pdf")
    expect(locations).toBe(true)
})

test("testing delete file functionality", async () => {
    let deleteResult = await deleteFile("invalid")
    expect(deleteResult).toBe(true)
    deleteResult = await deleteFile("tester.pdf")
    expect(deleteResult).toBe(true)
    await new Promise((resolve) => setTimeout(() => resolve(true), 10000)) // so master can reflect changes
    let locations = await getSingleChunkLocations("tester.pdf", 1)
    expect(locations).toEqual([])
})

