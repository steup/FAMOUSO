BOOST_VERSION=boost_1_36_0
rm -rf Boost boost*

wget http://mesh.dl.sourceforge.net/sourceforge/boost/${BOOST_VERSION}.tar.bz2
tar xfj ${BOOST_VERSION}.tar.bz2
mv ${BOOST_VERSION} Boost
