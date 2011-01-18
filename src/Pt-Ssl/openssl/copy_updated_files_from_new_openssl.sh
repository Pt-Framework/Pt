PT_OPENSSL_DIR=ptopenssl
NEW_OPENSSL_DIR=newopenssl
DEST_DIR=resultopenssl

FILE=`diff -rq $PT_OPENSSL_DIR $NEW_OPENSSL_DIR | grep differ | awk '{print $4}'`

rm -rvf $DEST_DIR

for I in $FILE; do
    oldIFS=$IFS
    IFS='/'
    TOKENS=($I)
    IFS=$oldIFS

    LEN=${#TOKENS[@]}
    let LEN1='LEN - 1'

    CUR_DIR=''
    for (( J = 1; J < $LEN1; ++J )); do
        CUR_DIR=$CUR_DIR/${TOKENS[J]}
    done
    echo $CUR_DIR/${TOKENS[LEN1]}
    mkdir -p $DEST_DIR$CUR_DIR
    cp $NEW_OPENSSL_DIR$CUR_DIR/${TOKENS[LEN1]} $DEST_DIR$CUR_DIR/${TOKENS[LEN1]}
done
