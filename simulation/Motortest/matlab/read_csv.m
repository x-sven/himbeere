



selection = 0;

if(1 == selection || 0 == selection)
    data_struct.file = '2012-10-23_3-2-1-1_F20A_geflashed.csv';
    data_struct.T_max_rpm  = 5;
    data_struct.T_min_rpm  = 7.5;
    data_struct.T_switch  = 11.375;

    if(selection)
        [time_vec, rpm_cmd_vec, rpm_state_vec] = full_dataset(data_struct);
    else
        [time_vec, rpm_cmd_vec, rpm_state_vec] = step_response(data_struct);
        time_vec = time_vec - time_vec(1);
    end
    plot(time_vec, rpm_cmd_vec, 'b--')
    hold on
    plot(time_vec, rpm_state_vec, 'r.-')
end
if(2 == selection || 0 == selection)
    data_struct.file = '2012-10-23_3-2-1-1_F20A_original.csv';
    data_struct.T_max_rpm  = 11;
    data_struct.T_min_rpm  = 13;
    data_struct.T_switch  = 12.125;

    if(selection)
        [time_vec, rpm_cmd_vec, rpm_state_vec] = full_dataset(data_struct);
    else
        [time_vec, rpm_cmd_vec, rpm_state_vec] = step_response(data_struct);
        time_vec = time_vec - time_vec(1);
    end
    plot(time_vec, rpm_state_vec, 'g.-')
    hold on
    %plot(time_vec, rpm_cmd_vec, 'b--')
end
if(3 == selection || 0 == selection)
    data_struct.file = '2012-10-23_3-2-1-1_HK25.csv';
    data_struct.T_max_rpm  = 11;
    data_struct.T_min_rpm  = 13;
    data_struct.T_switch  = 12;

    if(selection)
        [time_vec, rpm_cmd_vec, rpm_state_vec] = full_dataset(data_struct);
    else
        [time_vec, rpm_cmd_vec, rpm_state_vec] = step_response(data_struct);                time_vec = time_vec - time_vec(1);
        time_vec = time_vec - time_vec(1);
    end
    plot(time_vec, rpm_state_vec, 'm.-')
    hold on
    %plot(time_vec, rpm_cmd_vec, 'b--')
end
legend('Command', 'F20A geflashed', 'F20A original', 'HK SS 25');
title('Step Response of Hobbyking DT-750 + EPP 10x4,7, 6000 --> 4000rpm')
xlabel('Time in Seconds')
ylabel('Normalized Response')
grid on
hold off