INSERT INTO Doctor (name, specialization) VALUES ('Dr. Smith', 'Cardiology');
INSERT INTO Doctor (name, specialization) VALUES ('Dr. Jones', 'Dermatology');

INSERT INTO Client (name, contact_info, additional_details) VALUES ('John Doe', 'john.doe@example.com', 'N/A');
INSERT INTO Client (name, contact_info, additional_details) VALUES ('Jane Doe', 'jane.doe@example.com', 'N/A');

INSERT INTO WorkHours (doctor_id, day_of_week, start_time, end_time) VALUES (1, 'Monday', '08:00', '12:00');

INSERT INTO WorkHours (doctor_id, day_of_week, start_time, end_time) VALUES (2, 'Tuesday', '13:00', '17:00');

INSERT INTO Slots (work_hours_id, start_time, end_time) VALUES (1, '08:00', '08:30');

INSERT INTO Slots (work_hours_id, start_time, end_time) VALUES (1, '08:30', '09:00');

INSERT INTO Slots (work_hours_id, start_time, end_time) VALUES (2, '13:00', '13:30');

INSERT INTO Slots (work_hours_id, start_time, end_time) VALUES (2, '14:00', '14:30');

INSERT INTO Appointment (doctor_id, client_id, appointment_time, status) VALUES (1, 1, '2024-03-10 08:00:00', 'scheduled');

INSERT INTO Appointment (doctor_id, client_id, appointment_time, status) VALUES (1, 2, '2024-03-10 08:30:00', 'scheduled');

INSERT INTO Appointment (doctor_id, client_id, appointment_time, status) VALUES (2, 1, '2024-03-11 14:00:00', 'scheduled');

SELECT 
    a.appointment_id,
    d.name AS doctor_name,
    d.specialization,
    c.name AS client_name,
    c.contact_info,
    a.appointment_time,
    a.status
FROM 
    Appointment a
JOIN 
    Doctor d ON a.doctor_id = d.doctor_id
JOIN 
    Client c ON a.client_id = c.client_id
ORDER BY 
    a.appointment_time;
