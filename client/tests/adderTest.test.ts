import { expect, test } from "bun:test";
import { adder } from "..";

test("testing add function", () => {
    expect(adder(3, 4)).toBe(7);
});
