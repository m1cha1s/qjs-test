import * as console from "console";

export class Greeter {
    constructor(name) {
        this._name = name;
    }

    greet() {
        console.log("Hello,", this._name);
    }
}