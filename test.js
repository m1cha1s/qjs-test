import * as console from "console";
import * as fs from "fs";
import { Greeter } from "../test2.js";

let test_file = new fs.File("../test.txt", "r");

let mGreeter = new Greeter("m1cha1s");

mGreeter.greet();

console.log(12);