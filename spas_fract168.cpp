#include "spas_fract168.hpp"

// Public functions
void spas_fract168_t::printAll() const{
    printf("<%c%" PRIx64 " %" PRIx32 " %c%" PRIx64 ">", (this->sign&0b1000)?'-':'+', this->big, this->offset, (this->sign&0b0001)?'-':'+', this->small);
}

double spas_fract168_t::getDouble() const{
    uint64_t temp = 0x01;
    uint64_t tb = this->big;
    double x = 0;
    for(int i=0; i<64; i++){
        x = x/2.0f;
        if(tb&temp){
            x += 0.5;
        }
        temp = temp << 1;   
    }
    if(this->sign&0b1000){
        x = -x;
    }
    return x;
}

void spas_fract168_t::printBinary() const{
    //
}

void spas_fract168_t::printSign() const{
    printf("<%c %c>", (this->sign&0b1000)?'-':'+', (this->sign&0b0001)?'-':'+');
}

void spas_fract168_t::printOffset() const{
    printf("<%" PRIx64 ">", this->offset);
}

void spas_fract168_t::printSO() const{
    printf("<%c %c %" PRIx64 ">", (this->sign&0b1000)?'-':'+', (this->sign&0b0001)?'-':'+', this->offset);
}

// Constructors
spas_fract168_t::spas_fract168_t(){
    this->sign = 0;
    this->big = 0;
    this->small = 0;
    this->offset = 0;
}

spas_fract168_t::spas_fract168_t(double t){
    uint64_t temp = 0x8000000000000000;
    this->big = 0;
    this->small = 0;
    this->offset = 0;
    if(t<0){
        this->sign = 0b1000;
        t *= -1;
    }
    else{
        this->sign = 0b0000;
    }
    for(int i=0; i<64; i++){
        if(t-0.5f>=0){
            this->big = this->big|temp;
            t = t-0.5f;
        }
        temp = temp >> 1;
        t = t*2.0f;
    }
}

spas_fract168_t::spas_fract168_t(uint8_t sign, uint64_t big, uint32_t offset, uint64_t small){
    this->sign = sign;
    this->big = big;
    this->small = small;
    this->offset = offset;
}

spas_fract168_t::spas_fract168_t(const spas_fract168_t& t){
    this->sign = t.sign;
    this->big = t.big;
    this->small = t.small;
    this->offset = t.offset;
}

// Assignment arithimatic operators
spas_fract168_t& spas_fract168_t::operator=(const spas_fract168_t& t){
    if(this == &t){return *this;}
    this->sign = t.sign;
    this->big = t.big;
    this->small = t.small;
    this->offset = t.offset;
    return *this;
}

spas_fract168_t& spas_fract168_t::operator+=(const spas_fract168_t& rhs){
    // printf("Addition! Signs: %d %d \n", this->sign, rhs.sign);
    if((this->sign&0b1000) == (rhs.sign&0b1000)){ // (a+b) / (-a-b) = -(a+b)
        unsigned char lb_sign = 0, ls_sign = 0, rb_sign = 0, rs_sign = 0;
        if(this->sign&0b1000){lb_sign = 1;}
        if(this->sign&0b0001){ls_sign = 1;}
        if(rhs.sign&0b1000){rb_sign = 1;}
        if(rhs.sign&0b0001){rs_sign = 1;}

        unsigned char big_sign = 0, small_sign = 0;
        uint32_t discard;

        full_fraction_addition(big_sign, this->big, discard, lb_sign, this->big, 0, rb_sign, rhs.big, 0);
        uint8_t carry = full_fraction_addition(small_sign, this->small, this->offset, ls_sign, this->small, this->offset, rs_sign, rhs.small, rhs.offset);

        if(this->small){
            unsigned long index = __builtin_clzll(this->small);
            this->small = this->small << index;
            this->offset += index;
        }
        else{
            this->offset = 0;
            small_sign = 0;
        }

        this->sign = big_sign<<3|small_sign;
        
        if(carry){
            if(carry==2){
                if(big_sign){this->big+=1;}
                else{this->big-=1;}
            }
            else{
                if(big_sign){this->big-=1;}
                else{this->big+=1;}
            }
        }

        // printf("Returning subtraction \n");
        return *this;
    }
    if(this->sign&0b1000){ // (-a+b) = b-a = -(a-b)
        // printf("Recursive addition (-a+b) = -(a-b) : %d %d\n", this->sign, rhs.sign);
        this->sign ^= 0b1001;
        *this-=rhs;
        this->sign ^= 0b1001;
        return *this;
    }
    else{ // (a+(-b)) = (a-b)
        // printf("Recursive addition (a+(-b)) = (a-b) : %d %d\n", this->sign, rhs.sign);
        spas_fract168_t temp = rhs;
        temp.sign ^= 0b1001;
        *this-=temp;
        return *this;
    }
}

spas_fract168_t& spas_fract168_t::operator-=(const spas_fract168_t& rhs){
    // printf("Subtration! Signs: %d %d \n", this->sign, rhs.sign);
    if((this->sign&0b1000) == (rhs.sign&0b1000)){ // (a-b) / ((-a)-(-b)) = -(a-b)
        unsigned char lb_sign = 0, ls_sign = 0, rb_sign = 0, rs_sign = 0;
        if(this->sign&0b1000){lb_sign = 1;}
        if(this->sign&0b0001){ls_sign = 1;}
        if(rhs.sign&0b1000){rb_sign = 1;}
        if(rhs.sign&0b0001){rs_sign = 1;}

        unsigned char big_sign = 0, small_sign = 0;
        uint32_t discard;

        full_fraction_subtraction(big_sign, this->big, discard, lb_sign, this->big, 0, rb_sign, rhs.big, 0);
        uint8_t carry = full_fraction_subtraction(small_sign, this->small, this->offset, ls_sign, this->small, this->offset, rs_sign, rhs.small, rhs.offset);

        if(this->small){
            unsigned long index = __builtin_clzll(this->small);
            this->small = this->small << index;
            this->offset += index;
        }
        else{
            this->offset = 0;
            small_sign = 0;
        }

        this->sign = big_sign<<3|small_sign;

        if(carry){
            if(carry==2){
                if(big_sign){this->big+=1;}
                else{this->big-=1;}
            }
            else{
                if(big_sign){this->big-=1;}
                else{this->big+=1;}
            }
        }

        // printf("Returning subtraction \n");
        return *this;
    }
    else{
        if(this->sign&0b1000){ // (-a-b) = -(a+b)
            // printf("Recursive subtraction (-a-b) = -(a+b) : %d %d\n", this->sign, rhs.sign);
            this->sign ^= 0b1001;
            *this+=rhs;
            this->sign ^= 0b1001;
            return *this;
        }
        else{ // (a-(-b)) = (a+b)
            // printf("Recursive subtraction (a-(-b)) = (a+b) : %d %d\n", this->sign, rhs.sign);
            spas_fract168_t temp = rhs;
            temp.sign ^= 0b1001;
            *this+=temp;
            return *this;
        }
    }
}

spas_fract168_t& spas_fract168_t::operator*=(const spas_fract168_t& rhs){
    unsigned char t_sign = 0, r_sign = 0, b_sign = 0, s_sign = 0;

    uint64_t sb = 0, ss = 0;
    uint32_t so = 0;
    fraction_multiply(this->small, rhs.small, sb, ss);

    unsigned long sb_index = __builtin_clzll(sb);
    unsigned long ss_index = __builtin_clzll(ss);

    if(sb_index<0){
        sb_index = 64;
    }
    if(ss_index<0){
        ss_index = 64;
    }

    if((this->sign&0b0001) == (rhs.sign&0b0001)){
        s_sign = 0b0000;
    }
    else{
        s_sign = 0b0001;
    }

    if(sb){
        if(sb_index != 0){
            sb = (sb<<sb_index) | (ss>>(64-sb_index));
        }
        so = (this->offset+rhs.offset)+64+sb_index;
    }
    else{
        if(ss){
            sb = ss<<ss_index;
            so = (this->offset+rhs.offset)+128+ss_index;
        }
        else{
            sb = 0;
            so = 0;
        }
    }

    spas_fract168_t s_small((s_sign?0b0001:0b0000), 0, so, sb);

    if(this->big || rhs.big){
        uint64_t big, small;
        uint64_t t_small, r_small, ns;
        uint32_t t_off = this->offset, r_off = rhs.offset, off = 0;
        fraction_multiply(this->big, rhs.big, big, small);
        
        if(small){
            unsigned long _index = __builtin_clzll(small);
            small = small << _index;
            off = _index;
        }

        if((this->sign&0b1000) == (rhs.sign&0b1000)){
            b_sign = 0b0000;
        }
        else{
            b_sign = 0b1001;
        }

        this->big = big;
        if(!big){
            b_sign = 0b0001&b_sign;
        }

        if(this->big){
            r_sign = (((this->sign>>3)&0b0001)!=(rhs.sign&0b0001));
            fraction_multiply(this->big, rhs.small, r_small, ns);
            unsigned long big_index = __builtin_clzll(r_small);
            unsigned long small_index = __builtin_clzll(ns);

            if(big_index<0){
                big_index = 64;
            }
            if(small_index<0){
                small_index = 64;
            }

            if(r_small){
                if(big_index != 0){
                    r_small = (r_small<<big_index) | (ns>>(64-big_index));
                }
                r_off = (rhs.offset)+big_index;
            }
            else{
                if(ns){
                    r_small = ns<<small_index;
                    r_off = (rhs.offset)+64+small_index;
                }
                else{
                    r_sign = 0;
                    r_small = 0;
                    r_off = 0;
                }
            }
        }
        else{
            r_small = 0;
            r_off = 0;
        }

        if(rhs.big){
            t_sign=((this->sign&0b0001)!=((rhs.sign>>3)&0b0001));

            fraction_multiply(rhs.big, this->small, t_small, ns);
            unsigned long big_index = __builtin_clzll(t_small);
            unsigned long small_index = __builtin_clzll(ns);

            if(big_index<0){
                big_index = 64;
            }
            if(small_index<0){
                small_index = 64;
            }

            if(t_small){
                if(big_index != 0){
                    t_small = (t_small<<big_index) | (ns>>(64-big_index));
                }
                t_off = (this->offset)+big_index;
            }
            else{
                if(ns){
                    t_small = ns<<small_index;
                    t_off = (this->offset)+64+small_index;
                }
                else{
                    t_sign = 0;
                    t_small = 0;
                    t_off = 0;
                }
            }
        }
        else{
            t_small = 0;
            t_off = 0;
        }

        spas_fract168_t t((t_sign?0b0001:0b0000), 0, t_off, t_small);
        spas_fract168_t r((r_sign?0b0001:0b0000), 0, r_off, r_small);
        spas_fract168_t a((b_sign?0b1001:0b0000), big, off, small);
        
        /*
        t.printAll(); printf(" + ");
        r.printAll(); printf(" + ");
        a.printAll(); printf(" = ");
        */

        *this = a+t+r+s_small;
    }
    else{
        *this = s_small;
    }
    
    return *this;
}

// Friend Operators
spas_fract168_t operator+(spas_fract168_t lhs, const spas_fract168_t& rhs)
{   
    return lhs += rhs;
}

spas_fract168_t operator-(spas_fract168_t lhs, const spas_fract168_t& rhs)
{
    return lhs -= rhs;
}

spas_fract168_t operator*(spas_fract168_t lhs, const spas_fract168_t& rhs)
{
    return lhs *= rhs;
}

spas_fract168_t operator-(const spas_fract168_t& rhs){
    spas_fract168_t temp = rhs;
    temp.sign ^= 0b1001;
    return temp;
}

spas_fract168_t operator<<(spas_fract168_t lhs, const uint32_t rhs){
    lhs.big = lhs.big << 1;
    if(lhs.offset>0){
        lhs.offset-=1;
        lhs.small = lhs.small << 1;
    }
    else{
        lhs.big = lhs.big | (lhs.small >> 63);
    }
    return lhs;
}

uint64_t reverse_64(uint64_t t){
    t = ((t&0xFFFF'FFFF'0000'0000)>>32)|((t&0x0000'0000'FFFF'FFFF)<<32);
    t = ((t&0xFFFF'0000'FFFF'0000)>>16)|((t&0x0000'FFFF'0000'FFFF)<<16);
    t = ((t&0xFF00'FF00'FF00'FF00)>> 8)|((t&0x00FF'00FF'00FF'00FF)<< 8);
    t = ((t&0xF0F0'F0F0'F0F0'F0F0)>> 4)|((t&0x0F0F'0F0F'0F0F'0F0F)<< 4);
    t = ((t&0xCCCC'CCCC'CCCC'CCCC)>> 2)|((t&0x3333'3333'3333'3333)<< 2);
    t = ((t&0xAAAA'AAAA'AAAA'AAAA)>> 1)|((t&0x5555'5555'5555'5555)<< 1);
    return t;
}

uint32_t reverse_32(uint32_t t){
    t = (t&0xFFFF0000)>>16|(t&0x0000FFFF)<<16;
    t = (t&0xFF00FF00)>> 8|(t&0x00FF00FF)<< 8;
    t = (t&0xF0F0F0F0)>> 4|(t&0x0F0F0F0F)<< 4;
    t = (t&0xCCCCCCCC)>> 2|(t&0x33333333)<< 2;
    t = (t&0xAAAAAAAA)>> 1|(t&0x55555555)<< 1;
    return t;
}

void fraction_multiply(uint64_t lhs, uint64_t rhs, uint64_t &big, uint64_t &small){
    __int128_t temp = (__int128_t)lhs*(__int128_t)rhs;
    big = (uint64_t)(temp>>64);
    small = (uint64_t)temp;
}

// Return 1 if overflowed, always ensure lhs's offset is smaller than rhs's
uint8_t fraction_addition(uint64_t &lhs, uint64_t rhs, uint32_t offset){
    uint64_t temp = lhs;
    if(offset<64 && offset>=0){
        lhs += rhs >> offset;
    }
    return (lhs<temp?1:0);
}

// Return 1 if underflowed, always ensure lhs's offset is smaller than rhs's
uint8_t fraction_subtraction(uint64_t &lhs, uint64_t rhs, uint32_t offset){
    uint64_t temp = lhs;
    if(offset<64 && offset>=0){
        lhs -= rhs >> offset;
    }
    return (lhs>temp?1:0);
}

uint8_t full_fraction_addition(unsigned char &sign, uint64_t &res, uint32_t &res_off, unsigned char l_sign, uint64_t lhs, uint32_t l_off, unsigned char r_sign, uint64_t rhs, uint32_t r_off){
    if(l_sign == r_sign){
        // printf("----------FINISHING LOOP!----------");
        if((l_off>r_off && rhs!=0)||(l_off==r_off && rhs>lhs)||(lhs==0)){
            std::swap(lhs, rhs);
            std::swap(l_off, r_off);
        }
        sign = l_sign;
        uint32_t off = r_off-l_off;
        if(rhs&&lhs){
            res = lhs;
            res_off = l_off;
            uint8_t carry = fraction_addition(res, rhs, off);
            if(carry && res_off > 0){
                res = res << 1;
                res_off -= 1;
                carry = 0;
            }
            return carry;
        }
        else{
            res = lhs;
            res_off = l_off;
            return 0;
        }
    }
    else{
        // printf("Calling Addition recursion!!!\n");
        if(r_sign){ // a+(-b) = a-b
            return full_fraction_subtraction(sign, res, res_off, l_sign, lhs, l_off, 1-r_sign, rhs, r_off);
        }
        else{ // -a+b = b-a
            return full_fraction_subtraction(sign, res, res_off, r_sign, rhs, r_off, 1-l_sign, lhs, l_off);
        }
    }
}

uint8_t full_fraction_subtraction(unsigned char &sign, uint64_t &res, uint32_t &res_off, unsigned char l_sign, uint64_t lhs, uint32_t l_off, unsigned char r_sign, uint64_t rhs, uint32_t r_off){
    if(l_sign == r_sign){
        // printf("----------FINISHING LOOP!----------");
        sign = l_sign;
        if((l_off>r_off && rhs!=0)||(l_off==r_off && rhs>lhs)||(lhs==0)){
            std::swap(lhs, rhs);
            std::swap(l_off, r_off);
            sign = 1-sign;
        }
        uint32_t off = r_off-l_off;
        if(rhs&&lhs){
            res = lhs;
            res_off = l_off;
            return fraction_subtraction(res, rhs, off);
        }
        else{
            res = lhs;
            res_off = l_off;
            return 0;
        }
    }
    else{
        // printf("Calling Subtraction Recursion!!!\n");
        if(r_sign){ // lhs-(-rhs) = lhs+rhs
            return full_fraction_addition(sign, res, res_off, l_sign, lhs, l_off, 1-r_sign, rhs, r_off);
        }
        else{ // -lhs-rhs = -(lhs+rhs) = (-lhs)+(-rhs)
            return full_fraction_addition(sign, res, res_off, l_sign, lhs, l_off, 1-r_sign, rhs, r_off);
        }
    }
}

// Deprecated fraction multiplication for debug usage only
void _fraction_multiply(uint64_t lhs, uint64_t rhs, uint64_t &big, uint64_t &small){
    big = 0; small = 0;
    if(rhs>lhs){
        std::swap(lhs, rhs);
    }
    if(rhs && lhs){
        uint64_t t = 0x8000'0000'0000'0000;
        unsigned long index = __builtin_clzll(rhs);
        t = t >> index;
        for(unsigned long i = index; i<64; i++){
            if(t&rhs){
                uint64_t ls = small; // Hold temporary variable for overflow checking for small
                small += lhs<<(63-i); // Computate small addition
                if(i!=63){
                    big += (lhs>>(i+1))+((ls>small)?0x1:0x0); // big += (lhs shift by i+1) + (carry from small)
                }
                else{
                    big += ((ls>small)?0x1:0x0); // big += (carry from small) since i+1 = 64, LHS>>64 = 0 but is undefined
                }     
            }
            t = t>>1;
        }
    }
}
