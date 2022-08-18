FImage *edge_darkening_2(
 FImage *srgb_color_image,
 FImage *srgb_color_image2,
 int n,
 float beta
);

FImage *edge_darkening(
 FImage *srgb_color_image,
 FImage *srgb_color_image2,
 float beta
);

void lerpColor(
 int rgb1[3],
 int rgb2[3],
 float amt,
 int rgb3[3]
);

FImage *modify_color(
 FImage *srgb_color_image,
 FImage *srgb_texture_image,
 float beta
);

FImage *modify_color_hsl(
 FImage *srgb_color_image,
 FImage *srgb_texture_image,
 float beta
);

FImage *modify_color_hsv(
 FImage *srgb_color_image,
 FImage *srgb_texture_image,
 float beta
);

FImage *paper_texture(
 FImage *srgb_color_image,
 char *filename_paper_texture,
 float beta
);

FImage *pigment_dispersion(
 FImage *srgb_color_image,
 int octaves,
 float persistence,
 float frequency0,
 float beta
);

void thewatercolorist_main(
 char *filename_image,
 char *filename_paper_texture,
 float beta_paper_texture,
 float beta_turbulent_flow,
 int n_edge_darkening,
 float beta_edge_darkening,
 char *filename_output_image
);

FImage *turbulent_flow(
 FImage *srgb_color_image,
 int octaves,
 float persistence,
 float frequency0,
 float beta
);
