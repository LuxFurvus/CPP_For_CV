CREATE TABLE Doctor (
    doctor_id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    specialization VARCHAR(255),
    UNIQUE (name, specialization)
);

CREATE TABLE Client (
    client_id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    contact_info TEXT NOT NULL,
    additional_details TEXT,
    UNIQUE (name, contact_info)
);

CREATE TABLE Appointment (
    appointment_id SERIAL PRIMARY KEY,
    doctor_id INT NOT NULL,
    client_id INT NOT NULL,
    appointment_time TIMESTAMP NOT NULL,
    status VARCHAR(255) NOT NULL,
    FOREIGN KEY (doctor_id) REFERENCES Doctor(doctor_id) ON DELETE CASCADE,
    FOREIGN KEY (client_id) REFERENCES Client(client_id) ON DELETE CASCADE,
    UNIQUE (doctor_id, appointment_time)
);

CREATE TABLE WorkHours (
    work_hours_id SERIAL PRIMARY KEY,
    doctor_id INT NOT NULL,
    day_of_week VARCHAR(10) NOT NULL,
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    FOREIGN KEY (doctor_id) REFERENCES Doctor(doctor_id) ON DELETE CASCADE,
    UNIQUE (doctor_id, day_of_week, start_time, end_time)
);

CREATE TABLE Slots (
    slot_id SERIAL PRIMARY KEY,
    work_hours_id INT NOT NULL,
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    FOREIGN KEY (work_hours_id) REFERENCES WorkHours(work_hours_id) ON DELETE CASCADE,
    UNIQUE (work_hours_id, start_time, end_time)
);
