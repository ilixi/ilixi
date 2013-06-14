wget ftp://xmlsoft.org/libxml2/libxml2-2.9.1.tar.gz
tar xvzf libxml2-2.9.1.tar.gz

wget http://ftp.gnome.org/pub/GNOME/sources/libsigc++/2.2/libsigc++-2.2.10.tar.bz2
tar xvjf libsigc++-2.2.10.tar.bz2

wget https://github.com/vault/fontconfig/archive/androidbuild.tar.gz -O fontconfig-android.tar.gz
mkdir -p fontconfig-android
tar xvzf fontconfig-android.tar.gz -C fontconfig-android --strip-components=1

