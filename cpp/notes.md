### Ubuntu 22.04 Dev Setup
Base packages in repos
```
sudo apt install cmake gcc g++ libboost-all-dev libhdf5-dev
```
Catch2 for testing HighFive
```
wget https://github.com/catchorg/Catch2/archive/refs/tags/v3.2.0.tar.gz
tar xzf v3.2.0.tar.gz
cd Catch2-3.2.0
cmake -B build -S .
cd build
make
sudo make install
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

### Tree
- holds many branches
- holds H5 group that the HDTree is stored under
- iterator for `for (auto entry : tree)` syntax
- reference to reader/writer/both so open/close entry is well-defined
