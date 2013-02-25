
--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

--
-- Name: plpgsql; Type: PROCEDURAL LANGUAGE; Schema: -; Owner: postgres
--

CREATE PROCEDURAL LANGUAGE plpgsql;


ALTER PROCEDURAL LANGUAGE plpgsql OWNER TO postgres;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: Constant; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "Constant" (
    "constantToken" character varying(600),
    "rawLineNum" integer,
    "dbFileID" integer
);


ALTER TABLE public."Constant" OWNER TO postgres;

--
-- Name: Feature; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "Feature" (
    "paramIDFKey" integer,
    "featID" integer NOT NULL,
    "featName" character varying(10),
    "featMedian" integer
);


ALTER TABLE public."Feature" OWNER TO postgres;

--
-- Name: File; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "File" (
    "dbFileID" integer NOT NULL,
    "filePath" character varying(60)
);


ALTER TABLE public."File" OWNER TO postgres;

--
-- Name: File_fileID_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE "File_fileID_seq"
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public."File_fileID_seq" OWNER TO postgres;

--
-- Name: File_fileID_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE "File_fileID_seq" OWNED BY "File"."dbFileID";


--
-- Name: File_fileID_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('"File_fileID_seq"', 179, true);


--
-- Name: FilteredFeature; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "FilteredFeature" (
    "paramIDFKey" integer,
    "featIDPKey" integer NOT NULL,
    "featName" character varying(10),
    "featMedian" integer
);


ALTER TABLE public."FilteredFeature" OWNER TO postgres;

--
-- Name: Function; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "Function" (
    "dbFcnID" integer NOT NULL,
    "startIdx" integer,
    "endIdx" integer,
    "dbHashValueFcn" integer,
    "FileIdFKey" integer,
    "startRawIdx" integer,
    "endRawIdx" integer
);


ALTER TABLE public."Function" OWNER TO postgres;

--
-- Name: Function_dbFcnID_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE "Function_dbFcnID_seq"
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public."Function_dbFcnID_seq" OWNER TO postgres;

--
-- Name: Function_dbFcnID_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE "Function_dbFcnID_seq" OWNED BY "Function"."dbFcnID";


--
-- Name: Function_dbFcnID_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('"Function_dbFcnID_seq"', 982, true);


--
-- Name: Import; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "Import" (
    "importToken" character varying(600),
    "rawLineNum" integer,
    "dbFileID" integer
);


ALTER TABLE public."Import" OWNER TO postgres;

--
-- Name: Inexact2Comb; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "Inexact2Comb" (
    "paramIDFKey" integer,
    "featAIDFKey" integer,
    "featBIDFKey" integer,
    "featAPresent" boolean,
    "featBPresent" boolean,
    "dbRegionID" text
);


ALTER TABLE public."Inexact2Comb" OWNER TO postgres;

--
-- Name: Parameter; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "Parameter" (
    "windowSize" integer,
    stride integer,
    "regNormLevel" integer,
    "dbParamID" integer NOT NULL
);


ALTER TABLE public."Parameter" OWNER TO postgres;

--
-- Name: Parameter_dbParamID_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE "Parameter_dbParamID_seq"
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public."Parameter_dbParamID_seq" OWNER TO postgres;

--
-- Name: Parameter_dbParamID_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE "Parameter_dbParamID_seq" OWNED BY "Parameter"."dbParamID";


--
-- Name: Parameter_dbParamID_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('"Parameter_dbParamID_seq"', 30, true);


--
-- Name: Region; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "Region" (
    "dbRegionID" bigint NOT NULL,
    "startIdx" integer,
    "endIdx" integer,
    "rawStartIdx" integer,
    "rawEndIdx" integer,
    "hashValue" integer,
    "FcnIDFkey" integer,
    "ParamIDFKey" integer
);


ALTER TABLE public."Region" OWNER TO postgres;

--
-- Name: Region_dbRegionID_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE "Region_dbRegionID_seq"
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public."Region_dbRegionID_seq" OWNER TO postgres;

--
-- Name: Region_dbRegionID_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE "Region_dbRegionID_seq" OWNED BY "Region"."dbRegionID";


--
-- Name: Region_dbRegionID_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('"Region_dbRegionID_seq"', 35265, true);


--
-- Name: String; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE "String" (
    "stringToken" character varying(600),
    "rawLineNum" integer,
    "dbFileID" integer
);


ALTER TABLE public."String" OWNER TO postgres;

--
-- Name: dbFileID; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "File" ALTER COLUMN "dbFileID" SET DEFAULT nextval('"File_fileID_seq"'::regclass);


--
-- Name: dbFcnID; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Function" ALTER COLUMN "dbFcnID" SET DEFAULT nextval('"Function_dbFcnID_seq"'::regclass);


--
-- Name: dbParamID; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Parameter" ALTER COLUMN "dbParamID" SET DEFAULT nextval('"Parameter_dbParamID_seq"'::regclass);


--
-- Name: dbRegionID; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Region" ALTER COLUMN "dbRegionID" SET DEFAULT nextval('"Region_dbRegionID_seq"'::regclass);


--
-- Data for Name: Constant; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "Constant" ("constantToken", "rawLineNum", "dbFileID") FROM stdin;
\.


--
-- Data for Name: Feature; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "Feature" ("paramIDFKey", "featID", "featName", "featMedian") FROM stdin;
\.


--
-- Data for Name: File; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "File" ("dbFileID", "filePath") FROM stdin;
\.


--
-- Data for Name: FilteredFeature; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "FilteredFeature" ("paramIDFKey", "featIDPKey", "featName", "featMedian") FROM stdin;
\.


--
-- Data for Name: Function; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "Function" ("dbFcnID", "startIdx", "endIdx", "dbHashValueFcn", "FileIdFKey", "startRawIdx", "endRawIdx") FROM stdin;
\.


--
-- Data for Name: Import; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "Import" ("importToken", "rawLineNum", "dbFileID") FROM stdin;
\.


--
-- Data for Name: Inexact2Comb; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "Inexact2Comb" ("paramIDFKey", "featAIDFKey", "featBIDFKey", "featAPresent", "featBPresent", "dbRegionID") FROM stdin;
\.


--
-- Data for Name: Parameter; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "Parameter" ("windowSize", stride, "regNormLevel", "dbParamID") FROM stdin;
\.


--
-- Data for Name: Region; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "Region" ("dbRegionID", "startIdx", "endIdx", "rawStartIdx", "rawEndIdx", "hashValue", "FcnIDFkey", "ParamIDFKey") FROM stdin;
\.


--
-- Data for Name: String; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "String" ("stringToken", "rawLineNum", "dbFileID") FROM stdin;
\.


--
-- Name: File_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY "File"
    ADD CONSTRAINT "File_pkey" PRIMARY KEY ("dbFileID");


--
-- Name: FilteredFeature_featIDPKey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY "FilteredFeature"
    ADD CONSTRAINT "FilteredFeature_featIDPKey" PRIMARY KEY ("featIDPKey");


--
-- Name: Function_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY "Function"
    ADD CONSTRAINT "Function_pkey" PRIMARY KEY ("dbFcnID");


--
-- Name: Parameter_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY "Parameter"
    ADD CONSTRAINT "Parameter_pkey" PRIMARY KEY ("dbParamID");


--
-- Name: Region_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY "Region"
    ADD CONSTRAINT "Region_pkey" PRIMARY KEY ("dbRegionID");


--
-- Name: InexactIndex; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX "InexactIndex" ON "Inexact2Comb" USING btree ("paramIDFKey", "featAIDFKey", "featBIDFKey", "featAPresent", "featBPresent");


--
-- Name: fki_; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX fki_ ON "Function" USING btree ("FileIdFKey");


--
-- Name: fki_FilteredFeature_paramIDFKey; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX "fki_FilteredFeature_paramIDFKey" ON "FilteredFeature" USING btree ("paramIDFKey");


--
-- Name: fki_Region_FcnIDFkey_fkey; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX "fki_Region_FcnIDFkey_fkey" ON "Region" USING btree ("FcnIDFkey");


--
-- Name: myIndex; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX "myIndex" ON "Inexact2Comb" USING btree ("featAIDFKey");


--
-- Name: myIndexsas; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX "myIndexsas" ON "Inexact2Comb" USING btree ("featAIDFKey", "featBIDFKey");


--
-- Name: myindex; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX myindex ON "Inexact2Comb" USING btree ("featAIDFKey");


--
-- Name: Constant_dbFileIDFkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Constant"
    ADD CONSTRAINT "Constant_dbFileIDFkey" FOREIGN KEY ("dbFileID") REFERENCES "File"("dbFileID") ON DELETE CASCADE;


--
-- Name: Feature_ParamIDFkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Feature"
    ADD CONSTRAINT "Feature_ParamIDFkey" FOREIGN KEY ("paramIDFKey") REFERENCES "Parameter"("dbParamID") ON DELETE CASCADE;


--
-- Name: FilteredFeature_paramIDFKey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "FilteredFeature"
    ADD CONSTRAINT "FilteredFeature_paramIDFKey" FOREIGN KEY ("paramIDFKey") REFERENCES "Parameter"("dbParamID") ON DELETE CASCADE;


--
-- Name: Function_FileIdFKey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Function"
    ADD CONSTRAINT "Function_FileIdFKey_fkey" FOREIGN KEY ("FileIdFKey") REFERENCES "File"("dbFileID") ON DELETE CASCADE;


--
-- Name: Import_dbFileIDFkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Import"
    ADD CONSTRAINT "Import_dbFileIDFkey" FOREIGN KEY ("dbFileID") REFERENCES "File"("dbFileID") ON DELETE CASCADE;


--
-- Name: Inexact2Comb_paramIDFKey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Inexact2Comb"
    ADD CONSTRAINT "Inexact2Comb_paramIDFKey" FOREIGN KEY ("paramIDFKey") REFERENCES "Parameter"("dbParamID") ON DELETE CASCADE;


--
-- Name: Region_FcnIDFkey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Region"
    ADD CONSTRAINT "Region_FcnIDFkey_fkey" FOREIGN KEY ("FcnIDFkey") REFERENCES "Function"("dbFcnID") ON DELETE CASCADE;


--
-- Name: Region_ParamIDFKey_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "Region"
    ADD CONSTRAINT "Region_ParamIDFKey_fkey" FOREIGN KEY ("ParamIDFKey") REFERENCES "Parameter"("dbParamID");


--
-- Name: String_dbFileIDFkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "String"
    ADD CONSTRAINT "String_dbFileIDFkey" FOREIGN KEY ("dbFileID") REFERENCES "File"("dbFileID") ON DELETE CASCADE;


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--
