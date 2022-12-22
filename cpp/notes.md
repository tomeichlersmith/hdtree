### Ubuntu 22.04 Dev Setup
Base packages in repos
```
sudo apt install cmake gcc g++ libboost-all-dev libhdf5-dev
```
HighFive
```
wget https://github.com/BlueBrain/HighFive/archive/refs/tags/v2.6.2.tar.gz
tar xzf v2.6.2.tar.gz
cd HighFive-2.6.2
cmake -B build -S .
cd build
make 
make test
sudo make install
```

### Check HDF5 Threadsafety
The following command prints out the configuration of the HDF5 library
that is installed on the system.  We are looking for the `Threadsafety`
line under `Features`. If it is `yes`, then we _should_ be able to trust
that the HDF5 library calls are safely serialized.
```
h5cc -showconfig
```

