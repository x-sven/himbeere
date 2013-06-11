function [time_vec, rpm_cmd_vec, rpm_state_vec] = full_dataset(data_struct)

signals = importdata(data_struct.file);


[Y, MO, D, H, M, S] = datevec(signals.textdata(3:end,1));
signals.Time = (60*60 *H + 60 *M +S)-(60*60 *H(1) + 60 *M(1) +S(1)) ; 


time_vec = signals.Time;

rpm_cmd = (signals.data(:,14 -1));
rpm_cmd =  rpm_cmd/  ((rpm_cmd(16* data_struct.T_max_rpm) - rpm_cmd(16* data_struct.T_min_rpm)) / ...
                      ( signals.data(16*data_struct.T_max_rpm ,9 -1)- signals.data(16*data_struct.T_min_rpm ,9 -1) )) ;
rpm_cmd_vec =  rpm_cmd-  (rpm_cmd(16* data_struct.T_min_rpm) - signals.data(16* data_struct.T_min_rpm ,9 -1));

rpm_state_vec  = signals.data(:,9 -1);


% plot(signals.Time, signals.data(:,9 -1), 'r.-')
% hold on

% plot(signals.Time, rpm_cmd, 'b--')
% ylabel(signals.textdata(2,9));

% grid on
% hold off


end