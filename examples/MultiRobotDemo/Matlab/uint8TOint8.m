function aux=uint8TOint8(a)
    if a<=255 & a>127
        aux=a-256;
    else
        if a<0
            error('wrong input!');
        else
            aux=a;
        end
    end