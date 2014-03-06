#!/bin/bash
bin/civicc programs/$1.cvc -o progsrc/$1.s | tee soldier/soldier1.out

sed -n '/------------------------------/,/------------------------------/p' soldier/soldier1.out | sed 's/------------------------------//;' > soldier/$1-soldier1.cvc
bin/civicc soldier/$1-soldier1.cvc -o progsrc/$1.s > soldier/soldier2.out
sed -n '/------------------------------/,/------------------------------/p' soldier/soldier2.out | sed 's/------------------------------//;' > soldier/$1-soldier2.cvc

echo ''
echo '< SOLDIER >'
diff soldier/$1-soldier1.cvc soldier/$1-soldier2.cvc
echo '< SOLDIER >'
