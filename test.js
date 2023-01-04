class Greeter {
    constructor(name) {
        this._name = name;
    }

    greet() {
        print("Hello,", this._name);
    }
}


const mGreeter = new Greeter("m1cha1s");
mGreeter.greet();