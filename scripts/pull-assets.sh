#!/bin/bash

set -e
#set -x

# This is a temporary fix till we fix the asset with new directory strucutre.
check_asic_asset()
{
  if [[ -d nic/model_sim/libs && -n "$(ls -A nic/model_sim/libs)" ]] ; then
    mkdir -p nic/sdk/model_sim/libs
    mv nic/model_sim/libs/* nic/sdk/model_sim/libs
    rm -rf nic/model_sim
  fi

  if [ -d nic/asic ] ; then
    cd nic
    files=$(find ./asic -name "*.so" | grep 'capri')
    for f in $files ; do
      cp --parents -u $f ./sdk/third-party
    done
    cd -
    rm -rf nic/asic
  fi
}

if cmp minio/VERSIONS minio/.VERSIONS.orig
then
    echo "minio/VERSIONS same as current. Skipping pull-assets."
    check_asic_asset
    exit 0
fi

echo "Deleting any stale files"
for fname in $(find minio -name '*.txt') 
do
  while IFS='' read -r f || [[ -n "$f" ]]; do
    # We use this expresion to remove any trailing "/*"
    # so that /sw/nic/buildroot/output/*
    # becomes /sw/nic/buildroot/output
    rm -rf $(echo "$f" | sed -e 's/\/\*\s*$//')
  done < $fname
done

echo "pulling assets"
for fname in $(find minio -name '*.txt') 
do
  name=$(basename $fname .txt)
  version=$(grep "${name}" minio/VERSIONS | awk '{ print $2 }')
  echo asset-pull ${name} ${version}
  asset-pull ${name} ${version} /dev/stdout | tar xz
  echo
done
check_asic_asset
cp minio/VERSIONS minio/.VERSIONS.orig
