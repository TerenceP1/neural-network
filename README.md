# neural-network
A neural network library project (don't use in your code and use something else) as proof I wrote the code

# Documentation

## ml namespace has everything

The `ml` namespace contains all the stuff and I recommend not using `using namespace ml;`

### Function ml::log

#### Syntax:
`void log(int errorlevel, string inp)`

Logs errorlevel, timestamp, and message

errorlevel:

0 is INFO, 1 is WARNING, 2 is ERROR, and 3 is DEBUG. Other errorlevels will throw an error

inp:

The message

#### Class ml::Device

Holds info about the current GPU/CPU

#### ml::Device::Device:

Syntax: `Device(bool gpu)`

gpu if true if you want GPU acceleration

#### ml::Device::setFunc

Syntax: `setFunc(string name, func fc, func dFc)`

name: Name of activation function

fc: Activation function

dFc: Derivative of activation function

#### ml::Device::func

Syntax: `struct func {string ker; float (*cpuF)(float in)};`

ker: C code to take the float inp and apply the activation/derivative of activation and output to float out (varibales already declared). Note, can't use variables/activation function names of ind, der, sum, diff, mlt, a, b, softmax, or any other OpenCL reserved keyword.

cpuF: A pointer to a activation function to take a float input and apply activation/derivative of activation and return the value

#### ml::Device::compile

No parameters. Compiles the kernel but will crash if the `ml::Device` was initialized in CPU mode. If called twice, the function will crash.