@HV
(pow(sat(1-max(sqrt(sq(xx-16)+sq(zz+48)),3.0)/20),1.4)*18-0.25*18*exp(0-sq(sqrt(sq(xx-16)+sq(zz+48))/2.0)))+(pow(sat(1-max(sqrt(sq(xx+22)+sq(zz+68)),3.9)/26),1.4)*26-0.25*26*exp(0-sq(sqrt(sq(xx+22)+sq(zz+68))/2.6)))+(pow(sat(1-max(sqrt(sq(xx-30)+sq(zz+90)),4.8)/32),1.4)*32-0.25*32*exp(0-sq(sqrt(sq(xx-30)+sq(zz+90))/3.2)))
@LAVA_A
t6 = sat((0.05-w)*1000)
t7 = min(floor(r1*3),2)
t8 = sat((t7-0.5)*1000)
t9 = sat((t7-1.5)*1000)
t3 = 16-38*t8+52*t9
t4 = 0-(48+20*t8+22*t9)
t10 = sat((r2-0.5)*1000)*2-1
t11 = t10*sqrt(max(t4*t4-t3*t3,0))
@LAVA_B
s0 = s0+(t3-s0)*t6
s1 = s1+(t5+0.4-s1)*t6
s2 = s2+(t11-s2)*t6
s5 = s5+(3-s5)*t6
s6 = s6*(1-t6)
@LAVA_F
t0 = sat((s5-2.5)*4)
t1 = frac(s7*20)
t2 = frac(t*0.3+t1*7)
t3 = t2*4.2
t4 = (12+6*frac(t1*53.1))*(1+0.8*kick+0.3*e)
t5 = 1+3.5*frac(t1*31.7)
t6 = frac(t1*97.3)*6.2832
t7 = t4*t3-4.5*t3*t3
x = x+t0*cos(t6)*t5*t3
y = y+t0*t7
z = z+t0*sin(t6)*t5*t3
b = b+t0*(sat(t7*1.5+0.5)*(1.8+1.6*kick)*(1-0.5*t2)-b)
c = c+t0*(0.97-c)
