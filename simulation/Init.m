setenv('MAKEFLAGS','');

%------------------------------------
% Simulation Proberties
%------------------------------------
dt_sim = 0.005;

%------------------------------------
% Actuator Proberties
%------------------------------------
act_latency = 1*dt_sim; % one step delay = minimum


%------------------------------------
% Thrust Proberties
%------------------------------------

% %Näherung für Auftrieb nach Strahltheorie
% %FA[N] = -0.8*rho*lb*Rb*omega^2*Caalpha*alpha
% %Dichte der Luft
rho = 1.26; %kg/m^3
% %Blatttiefe
% lb = 0.01; %m
% %Blattlänge
Rb = 0.25/2; %m
% %Anstellwinkel
% alpha = 5*pi/180; %rad
% %Wirkungsgrad 
% eta = 0.8; %1 = 100%
% %Auftriebsbeiwert
% Caalpha = 2*pi;
% KA=0.8*rho*lb*Rb*Caalpha*alpha;
% ka1 = 0.8*2*pi*alpha*rho*Rb*2*Rb*lb;


%Moment infolge Schub
%MD = P/omega; P = FA/etaP*sqrt(F(2rhoA); A= pi*Rb^2
etaP = 0.75;

%Abmessungen und Masse
%Durchmesser
D = 1; %m
%Höhe (mittlere)
H = 0.03; %m
%Masse
m = 1; %kg
%Abstand der Rotoren (~D-2*Rb)
%L = D-2*Rb;
L = 0.5;

%Faktor omega^2->Fi
%k = (m*9.81/4)/(3000/60*2*pi)^2;
k = 0.0000192;
max_rpm = 7000*pi/30;
min_rpm = 0; % 2*pi*2200/60;

%Faktor domega -> M
b = 1e-6;

%Trägheitstensor
J = diag([0.25*m*(D/2)^2 0.25*m*(D/2)^2 0.5*m*(D/2)^2]);

%Gravitationsbeschleunigung
gvec = [0 0 9.81]';

%Interface
Klat = .001;
Klon = .001;
Kped = .01;

%KK
%ui = col*(KK * [lat lon ped]' +[1 1 1]')
KK = [Klat 0     Kped;...
        0  Klon -Kped;...
     -Klat 0     Kped;...
        0  -Klon -Kped];
    
%------------------------------------
% Reference Position (ScenarioWorld)
%------------------------------------
% Other Constants:
DegToRad = pi/180;
RadToDeg = 1/DegToRad;

% disp('   Hillerse');
% dRefLat      = 52.41999*DegToRad;    %latitude 52.4195
% dRefLon      = 10.4380*DegToRad;    %longitude 10.4359
% dRefAlt      = 63.98;   %height over elipsoid in m


vis.IP_fg = '127.0.0.1';
vis.PORT_fg = 5507;
vis.dt_vis = 0.02;

DegToRad = pi/180;
scenario.dRefLat = 52.319783*DegToRad;
scenario.dRefLon = 10.566620*DegToRad;
scenario.dRefAlt = 84.85;
