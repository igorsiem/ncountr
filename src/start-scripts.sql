-- SQLite
CREATE TABLE document_info (
    id INTEGER PRIMARY KEY
    , name TEXT
    , description TEXT
);

-- SQLite
INSERT INTO document_info (id, name, description)
VALUES (1, 'Doc Name', 'The Document...');

-- SQLite
SELECT * FROM document_info;