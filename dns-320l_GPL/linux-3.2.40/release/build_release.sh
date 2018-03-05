#
# Build Armada XP/370 release script
# Usage ./build_release.sh <LSP Ver> <Lernel Ver>
#

echo "DSMP LSP - Build STD release $1 based on kernel $2 - Starting..."
mkdir ./${1}
echo "DSMP LSP - Prepare kernel and kernel+LSP tar balls"
cd ../
git archive --format=tar --prefix=linux-${2}-${1}/ HEAD | gzip > linux-${2}-${1}.tgz
git archive --format=tar --prefix=linux-${2}/ v${2} | gzip > linux-${2}.tgz
cd release
echo "DSMP LSP - Extract and copy kernel files"
tar -xzf ../linux-${2}-${1}.tgz -C ./${1}
tar -xzf ../linux-${2}.tgz -C ./${1}
rm ../linux-${2}-${1}.tgz
rm ../linux-${2}.tgz
rm -fr ./${1}/linux-${2}-${1}/release
echo "DSMP LSP - Separate LSP files"
cd ${1}
perl ../get_mv_release.sh ./linux-${2}-${1} ./linux-${2}
cd linux-${2}-${1}_release
zip -r -q ../linux-${2}-${1}_release.zip ./linux-${2}-${1}/
cd ../../
echo "DSMP LSP - Copy release config files"
cp ./configs/* ./${1}/linux-${2}-${1}/arch/arm/configs
cd ./${1}/linux-${2}-${1}/cesa/openswan/
wget http://download.openswan.org/openswan/openswan-2.6.37.tar.gz
tar -zxvf openswan-2.6.37.tar.gz
cd openswan-2.6.37
patch -p1 < ../0001-mv_openswan_2_6_37.patch
cd ../../../../../
echo "DSMP LSP - Compile external binaries"
source ./make_release ${1} $2 binaries_external 370_v7up 370_v7up_nat 370_v7up_nas 370_v7up_stat xp_v7smp xp_v7smp_nat xp_v7smp_lpae xp_v7smp_lpae_stat xp_v7smp_stat xp_v7smp_nas xp_v7smp_be8 xp_v7amp_g1 xp_v7amp_g0
echo "DSMP LSP - Remove third party SLIC source code"
source ./make_export ${1} $2 binaries_external
echo "DSMP LSP - Cleanup"
rm ./${1}/linux-${2}-${1}_release.zip
rm -fr ./${1}/linux-${2}-${1}_release
rm -fr ./${1}/linux-${2}-${1}
rm -fr ./${1}/linux-${2}
echo "DSMP LSP - Done!"
