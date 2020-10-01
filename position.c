
void get_chunk_pos(int px, int py, int pz, int *cx, int *cy, int *cz)
{
	*cx = px / 16;
	*cy = py / 16;
	*cz = pz / 16;
}
