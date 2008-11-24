BOOST_VERSION=boost_1_36_0
rm -rf Boost boost*

wget http://prdownloads.sourceforge.net/sourceforge/boost/${BOOST_VERSION}.tar.bz2 || exit
tar xfj ${BOOST_VERSION}.tar.bz2
mv ${BOOST_VERSION} Boost
