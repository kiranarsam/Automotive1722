# Docker Setup
Use docker container to communicated between containers

## Build Docker Image
```bash
build . -t ubuntu_test_can
```
## Run application container
```bash
docker run -v ./:/app -w /app -i -t --name can-application -d ubuntu_test_can
```

Executing above command to run in background process.
-v ./:/app  ==> map volume mount from current directory on host machine with contianer path (/app)

## Run ethernet simulator container
```bash
docker run -v ./:/app -w /app -i -t --name ethernet-simulation --cap-add=NET_ADMIN  -d ubuntu_test_can
```

Executing above command to run in background process.
-v ./:/app  ==> map volume mount from current directory on host machine with contianer path (/app)

## Open multiple terminal session if required for the respective container:
Example:
```bash
docker exec -it can-application bash
docker exec -it ethernet-simulation bash
```

## Create a virtual CAN interface - vcan0 or vcan1
```bash
ip link add dev vcan0 type vcan
ip link set vcan0 up
ip link add dev vcan1 type vcan
ip link set vcan1 up
```

## Execute Applications (containers: can-application, ethernet-simulation)
```bash
./application
```