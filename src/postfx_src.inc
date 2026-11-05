fx_src:
 db "uniform sampler2D t;uniform vec2 r;uniform vec4 a;uniform vec4 b;uniform vec4 c;",10
 db "vec2 p;float as;float S;float K;float tm;",10
 db "vec3 T(vec2 u,float o){vec2 q=clamp(u,p*0.5,vec2(1.0)-p*0.5);return texture2D(t,vec2(q.x*0.5+o,q.y)).rgb;}",10
 db "vec3 fx(float o,float W,float C,vec2 uv){",10
 db "if(W>0.5&&W<1.5){uv+=S*0.012*vec2(sin(uv.y*38.0+tm*7.0),sin(uv.x*31.0+tm*6.0));}",10
 db "else if(W>1.5&&W<2.5){vec2 e=uv-vec2(0.5);e.x*=as;float an=S*0.30*(1.0-smoothstep(0.0,0.75,length(e)));",10
 db "e=vec2(cos(an)*e.x-sin(an)*e.y,sin(an)*e.x+cos(an)*e.y);e.x/=as;uv=e+vec2(0.5);}",10
 db "else if(W>2.5&&W<3.5){uv+=0.007*vec2(sin(uv.y*24.0+tm*1.3),cos(uv.x*19.0+tm*1.1));}",10
 db "else if(W>3.5&&W<4.5){vec2 e=uv-vec2(0.5);e.x*=as;float r2=dot(e,e);e*=1.0-0.35*r2;e.x/=as;uv=e+vec2(0.5);}",10
 db "else if(W>4.5){uv=vec2(0.5)+(uv-vec2(0.5))*(1.0-0.015-0.05*K);}",10
 db "if(a.w>1.0){vec2 q=vec2(a.w,a.w*r.y/r.x);uv=(floor(uv*q)+0.5)/q;}",10
 db "vec3 col=T(uv,o);",10
 db "if(C<0.5){vec2 d=vec2(0.5)-uv;float z=0.03+a.x*1.4;col=vec3(0.0);float s=0.0;",10
 db "for(int i=0;i<7;i++){float f=float(i)/6.0;float w=1.0-0.45*f;col+=w*T(uv+d*f*z,o);s+=w;}col/=s;}",10
 db "else if(C<1.5){vec2 mx=vec2(5.0*p.x,0.0);vec2 mu=vec2(0.0,5.0*p.y);",10
 db "float e=length(T(uv+mu,o)-T(uv-mu,o))+length(T(uv+mx,o)-T(uv-mx,o));",10
 db "vec3 bl=col+T(uv+mx,o)+T(uv-mx,o)+T(uv+mu,o)+T(uv-mu,o)+T(uv+2.0*mx,o)+T(uv-2.0*mx,o)+T(uv+2.0*mu,o)+T(uv-2.0*mu,o);",10
 db "col=bl*0.111*(0.75+3.5*e);}",10
 db "else if(C<2.5){vec2 mx=vec2(4.0*p.x,0.0);vec2 mu=vec2(0.0,4.0*p.y);",10
 db "float e=length(T(uv+mu,o)-T(uv-mu,o))+length(T(uv+mx,o)-T(uv-mx,o));",10
 db "col=col*0.75+min(1.0,e*0.9)*vec3(1.0,0.82,0.5);}",10
 db "else if(C<3.5){vec2 d=vec2((0.0015+0.008*K)*0.5,0.0);col=vec3(0.0);",10
 db "for(int i=-5;i<=5;i++){col+=T(uv+d*float(i),o);}col*=0.0909;col=floor(col*50.0+0.5)/50.0;}",10
 db "else if(C<4.5){float cs=max(6.0,r.y/90.0);vec2 cl=floor(gl_FragCoord.xy/cs);vec2 cc=(cl+0.5)*cs;",10
 db "vec3 hc=T(cc/r,o);float lm=dot(hc,vec3(0.33));float rd=cs*0.75*sqrt(clamp(lm*1.6,0.0,1.0));",10
 db "float m=1.0-smoothstep(rd-1.0,rd,length(gl_FragCoord.xy-cc));col=mix(col,hc*1.4*m,0.85);}",10
 db "else if(C<5.5){float lm=dot(col,vec3(0.33));float e=(0.004+0.012*(K+S))*(0.3+lm);",10
 db "col=vec3(T(uv+vec2(1.0,0.5)*e,o).r,col.g,T(uv-vec2(1.0,0.5)*e,o).b);}",10
 db "else if(C<6.5){col=floor(col*9.0+0.5)/9.0;}",10
 db "else if(C<7.5){vec3 dd=3.0*abs(col-T(uv+vec2(0.008,0.0),o));col=col*0.55+dd*vec3(1.0,0.5,0.3)*0.8;}",10
 db "else if(C<8.5){vec2 d=0.0035*vec2(cos(tm*0.6),sin(tm*0.6));col=vec3(0.0);",10
 db "for(int i=-4;i<=4;i++){col+=T(uv+d*float(i),o);}col*=0.111*1.15;}",10
 db "else{vec3 bl=col+T(uv+vec2(10.0*p.x,0.0),o)+T(uv-vec2(10.0*p.x,0.0),o)+T(uv+vec2(0.0,10.0*p.y),o)+T(uv-vec2(0.0,10.0*p.y),o)",10
 db "+T(uv+vec2(21.0*p.x,0.0),o)+T(uv-vec2(21.0*p.x,0.0),o)+T(uv+vec2(0.0,21.0*p.y),o)+T(uv-vec2(0.0,21.0*p.y),o)",10
 db "+T(uv+vec2(42.0*p.x,0.0),o)+T(uv-vec2(42.0*p.x,0.0),o)+T(uv+vec2(0.0,42.0*p.y),o)+T(uv-vec2(0.0,42.0*p.y),o);",10
 db "col=col*0.78+bl*0.065;}",10
 db "vec3 g=vec3(0.0);",10
 db "g+=T(uv+vec2(4.0*p.x,0.0),o)+T(uv-vec2(4.0*p.x,0.0),o);",10
 db "g+=T(uv+vec2(0.0,4.0*p.y),o)+T(uv-vec2(0.0,4.0*p.y),o);",10
 db "g+=T(uv+vec2(3.0*p.x,3.0*p.y),o)+T(uv-vec2(3.0*p.x,3.0*p.y),o);",10
 db "g+=T(uv+vec2(3.0*p.x,-3.0*p.y),o)+T(uv-vec2(3.0*p.x,-3.0*p.y),o);",10
 db "vec3 h=vec3(0.0);",10
 db "h+=T(uv+vec2(12.0*p.x,0.0),o)+T(uv-vec2(12.0*p.x,0.0),o);",10
 db "h+=T(uv+vec2(0.0,12.0*p.y),o)+T(uv-vec2(0.0,12.0*p.y),o);",10
 db "h+=T(uv+vec2(9.0*p.x,9.0*p.y),o)+T(uv-vec2(9.0*p.x,9.0*p.y),o);",10
 db "h+=T(uv+vec2(9.0*p.x,-9.0*p.y),o)+T(uv-vec2(9.0*p.x,-9.0*p.y),o);",10
 db "col+=a.y*(g*0.0625+h*0.0625);",10
 db "return col;}",10
 db "void main(){",10
 db "vec2 uv=gl_FragCoord.xy/r;p=1.0/r;as=r.x/r.y;S=b.x;K=b.y;tm=b.w;",10
 db "vec3 bg=fx(0.0,c.z,c.w,uv);bg=min(vec3(1.0),pow(bg,vec3(0.78))*1.3);",10
 db "vec3 col=fx(0.5,c.x,c.y,uv)+bg;",10
 db "if(a.z>1.0)col=floor(col*a.z+0.5)/a.z;",10
 db "gl_FragColor=vec4(col,1.0);}",10
 db 0
