close all;
clear all;

e_fcci = 1;
e_ecf  = 2;
e_imu  = 3;

files  = {  
         {['01_circle', filesep, '_0_logfile_fcci.log']}, ...
            {['01_circle', filesep, '_0_logfile_ecf.log' ]}, ...
                {['01_circle', filesep, '_0_logfile_drotek10.log']}; ...
         {['02_circle_corrected', filesep, '_0_logfile_fcci.log']}, ...
            {['02_circle_corrected', filesep, '_0_logfile_ecf.log' ]}, ...
                {['02_circle_corrected', filesep, '_0_logfile_drotek10.log']}; ...
         {['03_circle_xaxis_down', filesep, '_0_logfile_fcci.log']}, ...
            {['03_circle_xaxis_down', filesep, '_0_logfile_ecf.log' ]}, ...
                {['03_circle_xaxis_down', filesep, '_0_logfile_drotek10.log']}; ...
         {['04_motor_halfthrust_north', filesep, '_0_logfile_fcci.log']}, ...
            {['04_motor_halfthrust_north', filesep, '_0_logfile_ecf.log' ]}, ...
                {['04_motor_halfthrust_north', filesep, '_0_logfile_drotek10.log']}; ...
         {['05_circle_again', filesep, '_0_logfile_fcci.log']}, ...
            {['05_circle_again', filesep, '_0_logfile_ecf.log' ]}, ...
                {['05_circle_again', filesep, '_0_logfile_drotek10.log']}; ...
         {['06_circle_xaxis_down', filesep, '_0_logfile_fcci.log']}, ...
            {['06_circle_xaxis_down', filesep, '_0_logfile_ecf.log' ]}, ...
                {['06_circle_xaxis_down', filesep, '_0_logfile_drotek10.log']}; ...
         {['07_motor_north', filesep, '_0_logfile_fcci.log']}, ...
            {['07_motor_north', filesep, '_0_logfile_ecf.log' ]}, ...
                {['07_motor_north', filesep, '_0_logfile_drotek10.log']}; ...       
         };

     
num_test=7;

fcci = importfile(files{num_test,e_fcci}{1});
ecf  = importfile(files{num_test,e_ecf}{1});
imu  = importfile(files{num_test,e_imu}{1});

% % Bei der fünften Datei haben wir offenbar die Spalten getauscht!? Muss wohl...
% if (num_test>0) && (num_test<5) %for num_test=5 only
%     real_col  = fcci.ch_2;
%     fcci.ch_2 = fcci.ch_1;
%     fcci.ch_1 = real_col;
% end

% 50 Hz FCCI-Daten auf 100 Hz IMU-Daten interpolieren
%yi = interp1(x,Y,xi);
ch_0_intp = interp1(fcci.Time,fcci.ch_0,imu.Time, 'linear', 0);
ch_1_intp = interp1(fcci.Time,fcci.ch_1,imu.Time, 'linear', 0);
ch_2_intp = interp1(fcci.Time,fcci.ch_2,imu.Time, 'linear', 0);
ch_3_intp = interp1(fcci.Time,fcci.ch_3,imu.Time, 'linear', 0);

% % Magnetometer Offset-Werte, ermittelt mit "02_drehungohnemotor"
% if(num_test<5) % nur Test 5 (hoffentlich) mit korregierten Werten aufgenommen!? 
    bx = -55;
    by = +140;
    bz = +50;
    
    sy = 1;%0.98;
    sz = 1;%1.02;
% else
%     bx = 0;
%     by = 0;
%     bz = 0;
% end

% Kreisdarstellung, 
if(num_test < 4 || num_test == 5 || num_test == 6 ) 
    
    % X-Y-Circle
    name = ['X-Y-Circle (num_test=', num2str(num_test), ')'];
    figure('Name', name )
    hold on
    radius = mean(sqrt((imu.my+by).^2+(imu.mx+bx).^2));
    phi = 0:0.1:2*pi;
    cy = radius*sin(phi);
    cx = radius*cos(phi);
    plot(imu.my,imu.mx,'r-');
    plot(imu.my+by,imu.mx+bx,'g-');
    plot(cy,cx, 'k--');
    xlabel('my')
    ylabel('mx')
    legend('raw', 'corrected', 'desired')
    grid on
    axis equal
    hold off
    
    % Y-Z-Circle
    name = ['Z-Y-Circle (num_test=', num2str(num_test), ')'];
    figure('Name', name )
    hold on
    radius = mean(sqrt((imu.my+by).^2+(imu.mz+bz).^2));
    theta = 0:0.1:2*pi;
    cy = radius*sin(theta);
    cz = radius*cos(theta);
    plot(imu.my,imu.mz,'r-');
    plot(sy*(imu.my+by),sz*(imu.mz+bz),'g-');
    plot(cy,cz, 'k--');
    xlabel('my')
    ylabel('mz')
    legend('raw', 'corrected', 'desired')
    grid on
    axis equal
    hold off

    % Magnitude, min, max, and mean
    name = ['Magnitude (X-Y-Circle, num_test=', num2str(num_test), ')'];
    figure('Name', name )
    hold on
    magnitude = sqrt((imu.mx+bx).^2+(imu.my+by).^2+(imu.mz+bz).^2);
    magnitude_min = min(magnitude);
    magnitude_max = max(magnitude);
    magnitude_mean = mean(magnitude);
    plot( magnitude, 'b-');
    plot( magnitude_mean*ones(length(magnitude)), 'k--', 'linewidth', 2 );
    plot( magnitude_min*ones(length(magnitude)), 'r--', 'linewidth', 2);
    plot( magnitude_max*ones(length(magnitude)), 'r--', 'linewidth', 2 );
    xlabel('indices')
    ylabel('magnitude (MAG)')
    legend('magnitude', 'mean', 'min, max')
    grid on
    hold off

    % Report relative difference from mean
    spread = (magnitude_max-magnitude_min)/magnitude_mean;
    fprintf('magnitude of magnetic field varies by: %.1f percent, wrt. to mean value.\n', spread*100);
end

% Messwerte um Bias korregieren
imu.mx = imu.mx+bx;
imu.my = imu.my+by;
 

% Trimmwerte für Kompensation der Steuerkanäle
ped_trim =  1500;
col_trim =  1000;
lon_trim =  1500;
lat_trim =  1500;

% % Skalierung d. Länge d.  Magnetfeldvektor mit throttle-Kanal (quadratisch)
% % if (num_test == 2)
% %     s_col = 6e-7;  % num_test = 2
% % elseif (num_test== 3 || num_test == 5)
% %     s_col = 1.3e-6; % num_test=3 & 5
% % else
% %     s_col = 0;
% % end
% s_col = 5e-7;
% scale = s_col.* ((ch_1_intp-col_trim).^2);

name = ['Magnitude (num_test=', num2str(num_test), ')'];
figure('Name', name )
 hold on
 grid on
%  mag_scale = sqrt((imu.mx).^2+(imu.my).^2+(imu.mz).^2)-480.;
%  plot(imu.Time, sqrt((imu.mx).^2+(imu.my).^2+(imu.mz).^2), 'r-');
%  plot(imu.Time, sqrt((imu.mx).^2+(imu.my).^2+(imu.mz).^2)-mag_scale, 'g-');
% hold off


% name = ['Magnitude2 (num_test=', num2str(num_test), ')'];
% figure('Name', name )
% hold on
% grid on
% plot(imu.Time, imu.mx,'r-');
% plot(imu.Time, imu.my,'g-');
% plot(imu.Time, imu.mz,'b-');
% plot(imu.Time,ch_0_intp/10, 'r.');
% plot(imu.Time,ch_1_intp/10, 'g.');
% plot(imu.Time,ch_2_intp/10, 'k.');
% plot(imu.Time,ch_3_intp/10, 'b.');
% xlabel('IMU Time in s')
% ylabel('MAG magnitude')
% legend('imu.mx', ...
%        'imu.my', ...
%        'imu.mz', ...
%        'ch0=ped', ...
%        'ch1=col', ...
%        'ch2=lon', ...
%        'ch3=lat');
% 
% imu.mx = imu.mx - scale.*imu.mx;
% imu.my = imu.my - scale.*imu.my;
% imu.mz = imu.mz - scale.*imu.mz;

plot(imu.Time, sqrt((imu.mx).^2+(imu.my).^2+(imu.mz).^2), 'k-');
plot(imu.Time,ch_0_intp/10, 'r-');
plot(imu.Time,ch_1_intp/10, 'g-');
plot(imu.Time,ch_2_intp/10, 'k-');
plot(imu.Time,ch_3_intp/10, 'b-');
xlabel('IMU Time in s')
ylabel('MAG magnitude')
legend('before...', ...
       'ch0=ped', ...
       'ch1=col', ...
       'ch2=lon', ...
       'ch3=lat');


% Richtungskorrektur mit Steuerkanälen
% "pedal": ohne Einfluss
c_xy_ped =  0/1;
c_xz_ped =  0/1;

% "throttle": quadratisch
c_xy_col =  6e-05;  
c_xz_col =  0/50;

% longitudinal: linear
c_xy_lon_pos =  1/500;  % li
c_xy_lon_neg =  1/50;
c_xz_lon     =  0/25;

% lateral: linear
c_xy_lat_pos =  1/80;
c_xy_lat_neg =  1/40;
c_xz_lat     =  0/25;


% input_rc_abs=sqrt((ch_1_intp-col_trim).^2).*1/1000 + ...
%                   sqrt((ch_2_intp-lon_trim).^2).*1/500 + ...
%                   sqrt((ch_3_intp-lat_trim).^2).*1/500 + ...
%                   sqrt((ch_0_intp-ped_trim).^2).*1/500;
% 
% comp_x = 0;
% comp_y = sqrt(9.*mag_scale.^2./13).*sqrt((ch_1_intp-col_trim).^2) .*1/1000  .* 1./input_rc_abs;
% comp_z = sqrt(9.*mag_scale.^2./10).*sqrt((ch_1_intp-col_trim).^2) .*1/1000  .* 1./input_rc_abs;
% 
% 
% name = ['Magnitude2 (num_test=', num2str(num_test), ')'];
% figure('Name', name )
% hold on
% plot(imu.Time, sqrt((imu.mx).^2+(imu.my).^2+(imu.mz).^2), 'r-');
% plot(imu.Time, sqrt((imu.mx).^2+(imu.my).^2+(imu.mz).^2)-mag_scale, 'g-');
% 
% imu.mx = imu.mx+comp_x;
% imu.my = imu.my+comp_y;
% imu.mz = imu.mz+comp_z;
% 
% hold off

name = ['Magnitude3 (num_test=', num2str(num_test), ')'];
figure('Name', name )
hold on
grid on
% plot(imu.Time, comp_x,'r--');
% plot(imu.Time, comp_y,'g--');
% plot(imu.Time, comp_z,'b--');



plot(imu.Time, imu.mx,'r-');
plot(imu.Time, imu.my,'g-');
plot(imu.Time, imu.mz,'b-');
plot(imu.Time,ch_0_intp/10, 'r.');
plot(imu.Time,ch_1_intp/10, 'g.');
plot(imu.Time,ch_2_intp/10, 'k.');
plot(imu.Time,ch_3_intp/10, 'b.');
xlabel('IMU Time in s')
ylabel('MAG magnitude')
legend('imu.mx', ...
       'imu.my', ...
       'imu.mz', ...
       'ch0=ped', ...
       'ch1=col', ...
       'ch2=lon', ...
       'ch3=lat');
hold off





% X-Y-Ebene
comp_xy = c_xy_ped*     sqrt((ch_0_intp-ped_trim).^2) + ... 
          c_xy_col*         ((ch_1_intp-col_trim).^2) + ... 
          c_xy_lon_pos*(sqrt((ch_2_intp-lon_trim).^2) + (ch_2_intp-lon_trim)) + ...
          c_xy_lon_neg*(sqrt((ch_2_intp-lon_trim).^2) - (ch_2_intp-lon_trim)) + ...
          c_xy_lat_pos*(sqrt((ch_3_intp-lat_trim).^2) + (ch_3_intp-lat_trim)) + ...
          c_xy_lat_neg*(sqrt((ch_3_intp-lat_trim).^2) - (ch_3_intp-lat_trim));  

% X-Z-Ebene (ohne Bedeutung?)      
comp_xz = sqrt( ...
           c_xz_ped*(ch_0_intp-ped_trim).^2 + ... 
           c_xz_col*(ch_1_intp-col_trim).^2 + ...
           c_xz_lon*(ch_2_intp-lon_trim).^2 + ...
           c_xz_lat*(ch_3_intp-lat_trim).^2 );       

for ii=1:length(comp_xy)

    psi     = -comp_xy(ii)*pi/180;
    theta   = -comp_xz(ii)*pi/180;
    Tz = [[cos(psi) ,sin(psi),0]; ...
          [-sin(psi),cos(psi),0]; ... 
          [0        ,       0,1]];
    Tx = [cos(theta),0,-sin(theta);0,1,0;sin(theta),0,cos(theta)] ;

    in_vec = [imu.mx(ii); imu.my(ii); imu.mz(ii)]; 
    out_vec(:,ii) =Tx*Tz*in_vec;
end

% Für die Darstellung: Bestimmung Mittelwert der ersten und letzten 100 Messwerte
desired_psi_initial = mean( -atan2(out_vec(2,1:100),out_vec(1,1:100)) )*180/pi;
desired_psi_final   = mean( -atan2(out_vec(2,end-100:end),out_vec(1,end-100:end)) )*180/pi;


% Ergebnis: Steuerkanäle/10, 
%           Heading basierend auf Mag-Messungen
%           Kompensationssignal in X-Y-Ebene
%           Kompensiertes Heading
%           +/- 5° Grenzen aus ersten 100 Messwerten

name = ['Results (num_test=', num2str(num_test), ')'];
figure('Name', name )
hold on
plot(imu.Time,ch_0_intp/10, 'r-');
plot(imu.Time,ch_1_intp/10, 'g-');
plot(imu.Time,ch_2_intp/10, 'k-');
plot(imu.Time,ch_3_intp/10, 'b-');
plot(imu.Time,-atan2(imu.my,imu.mx)*180/pi, 'k*');
plot(imu.Time,comp_xy, 'r+');
%plot(imu.Time,(-atan2(imu.my,imu.mx)*180/pi)-comp_xy, 'g--');
plot(imu.Time,-atan2(out_vec(2,:),out_vec(1,:))*180/pi, 'm-');
plot(imu.Time(1:end),    (desired_psi_initial+5)*ones(length(imu.Time(1:end))), 'c-', 'linewidth', 1);
plot(imu.Time(1:end),    (desired_psi_initial-5)*ones(length(imu.Time(1:end))), 'c-', 'linewidth', 1);
plot(imu.Time(1:end),   +(desired_psi_final+5)*ones(length(imu.Time(1:end))),   'c-', 'linewidth', 1);
plot(imu.Time(1:end),   +(desired_psi_final-5)*ones(length(imu.Time(1:end))),   'c-', 'linewidth', 1);
grid on
legend('ch0=ped', ...
       'ch1=col', ...
       'ch2=lon', ...
       'ch3=lat', ...
       'mag-psi', ...
       'compensation-xy', ...
       'psi-xy-comp', ...
       'psi-comp', ...
       '5-deg-limits');
hold off
 

% figure
% hold on
% plot(imu.Time,ch_0_intp/10, 'r-');
% plot(imu.Time,ch_1_intp/10, 'g-');
% plot(imu.Time,ch_2_intp/10, 'k-');
% plot(imu.Time,ch_3_intp/10, 'b-');
% plot(imu.Time,-atan2(imu.mz,sqrt(imu.mx.^2+imu.my.^2))*180/pi, 'k*');
% %plot(ecf.Time,ecf.psi_deg, 'k--');
% plot(imu.Time,comp_xz, 'r+');
% plot(imu.Time,-atan2(imu.mz,sqrt(imu.mx.^2+imu.my.^2))*180/pi-comp_xz, 'g--');
% plot(imu.Time,-atan2(out_vec(3,:),sqrt(out_vec(1,:).^2+out_vec(2,:).^2))*180/pi, 'm-');
% grid on
% hold off





