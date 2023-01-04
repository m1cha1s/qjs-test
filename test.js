import * as console from "console";
import { Greeter } from "../test2.js";

let mGreeter = new Greeter("m1cha1s");

for (let i = 0; i < 10; i++) {
    mGreeter.greet();
}
console.log(12);